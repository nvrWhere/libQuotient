// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "e2ee_common.h"

#include "../logging_categories_p.h"

#include <QtCore/QRandomGenerator>

#include <openssl/crypto.h>

using namespace Quotient;

QByteArray Quotient::byteArrayForOlm(size_t bufferSize)
{
    if (bufferSize < std::numeric_limits<QByteArray::size_type>::max())
        return { static_cast<QByteArray::size_type>(bufferSize), '\0' };

    qCritical(E2EE) << "Too large buffer size:" << bufferSize;
    // Zero-length QByteArray is an almost guaranteed way to cause
    // an internal error in QOlm* classes, unless checked
    return {};
}

auto initializeSecureHeap()
{
#if !defined(LIBRESSL_VERSION_NUMBER)
    const auto result =
        CRYPTO_secure_malloc_init(FixedBufferBase::TotalSecureHeapSize, 16);
    if (result > 0) {
        qDebug(E2EE) << FixedBufferBase::TotalSecureHeapSize
                     << "bytes of secure heap initialised";
        if (std::atexit([] {
                CRYPTO_secure_malloc_done();
                qDebug(E2EE) << "Dismantled secure heap";
            })
            != 0)
            qWarning(E2EE)
                << "Could not register a cleanup function for secure heap!";
    } else
        qCritical(E2EE) << "Secure heap could not be initialised, sensitive "
                           "data will remain in common dynamic memory";
#else
    const auto result = 0;
    qCritical(E2EE) << "Secure heap is not available in LibreSSL";
#endif
    return result;
}

uint8_t* allocate(size_t bytes, bool initWithZeros = false)
{
#if !defined(LIBRESSL_VERSION_NUMBER)
    static auto secureHeapInitialised [[maybe_unused]] = initializeSecureHeap();

    const auto p = static_cast<uint8_t*>(initWithZeros
                                             ? OPENSSL_secure_zalloc(bytes)
                                             : OPENSSL_secure_malloc(bytes));
    Q_ASSERT(CRYPTO_secure_allocated(p));
    qDebug(E2EE) << "Allocated" << CRYPTO_secure_actual_size(p)
                 << "bytes of secure heap (requested" << bytes << "bytes),"
                 << CRYPTO_secure_used()
                 << "/ 65536 bytes of secure heap used in total";
#else
    const auto p = static_cast<uint8_t*>(initWithZeros
                                             ? calloc(bytes, 1)
                                             : malloc(bytes));
#endif
    return p;
}

FixedBufferBase::FixedBufferBase(size_t bufferSize, InitOptions options)
    : size_(bufferSize)
{
    if (bufferSize >= TotalSecureHeapSize) {
        qCritical(E2EE) << "Too large buffer size:" << bufferSize;
        return;
    }
    if (options == Uninitialized)
        return;

    data_ = allocate(size_, options == FillWithZeros);
    if (options == FillWithRandom) {
        // QRandomGenerator::fillRange works in terms of 32-bit words,
        // and FixedBuffer happens to deal with sizes that are multiples
        // of those (16, 32, etc.)
        const qsizetype wordsCount = size_ / 4;
        QRandomGenerator::system()->fillRange(
            reinterpret_cast<uint32_t*>(data_), wordsCount);
        if (const auto remainder = size_ % 4; Q_UNLIKELY(remainder != 0)) {
            // Not normal; but if it happens, apply best effort
            // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            const auto end = data_ + size_;
            QRandomGenerator::system()->generate(end - remainder, end);
            // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        }
    }
}

void FixedBufferBase::fillFrom(QByteArray&& source)
{
    if (unsignedSize(source) != size_) {
        qCritical(E2EE) << "Can't load a fixed buffer of length" << size_
                        << "from a string with length" << source.size();
        Q_ASSERT(unsignedSize(source) == size_); // Always false
        return;
    }
    if (data_ != nullptr) {
        qWarning(E2EE) << "Overwriting the fixed buffer with another string";
        clear();
    }

    data_ = allocate(size_);
    std::copy(source.cbegin(), source.cend(), reinterpret_cast<char*>(data_));
    if (source.isDetached())
        source.clear();
    else
        qWarning(E2EE)
            << "The fixed buffer source is shared; assuming that the caller is "
               "responsible for securely clearing other copies";
}

void FixedBufferBase::clear()
{
    if (empty())
        return;

#if !defined(LIBRESSL_VERSION_NUMBER)
    Q_ASSERT(CRYPTO_secure_allocated(data_));
    const auto actualSize = OPENSSL_secure_actual_size(data_);
    OPENSSL_secure_clear_free(data_, size_);
    qDebug(E2EE) << "Deallocated" << actualSize << "bytes,"
                 << CRYPTO_secure_used() << "/ 65536 bytes of secure heap used";
#else
    free(data_);
#endif
    data_ = nullptr;
}
