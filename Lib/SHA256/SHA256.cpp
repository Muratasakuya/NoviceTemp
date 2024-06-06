#include "SHA256.h"

/*===============================================================================*/
///
///									コンストラクタ
///
/*===============================================================================*/
SHA256::SHA256() : bufferSize_(0), bitLength_(0) {

	h = {
		0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
		0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
	};
}

/*===============================================================================*/
///
///									ハッシュ変換
///
/*===============================================================================*/
void SHA256::Transform(const uint8_t* chunk) {

	static constexpr uint32_t k[64] = {
		// First 32 bits of the fractional parts of the cube roots of the first 64 primes 2..311:
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
		0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
		0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
		0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
		0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
		0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
		0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
		0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
		0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
	};

	auto rotr = [](uint32_t x, uint32_t n) { return (x >> n) | (x << (32 - n)); };

	uint32_t w[64];
	for (int i = 0; i < 16; ++i) {
		w[i] = (chunk[i * 4] << 24) | (chunk[i * 4 + 1] << 16) | (chunk[i * 4 + 2] << 8) | chunk[i * 4 + 3];
	}
	for (int i = 16; i < 64; ++i) {
		w[i] = w[i - 16] + (rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3)) + w[i - 7] + (rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10));
	}

	uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
	uint32_t e = h[4], f = h[5], g = h[6], h_ = h[7];

	for (int i = 0; i < 64; ++i) {
		uint32_t t1 = h_ + (rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25)) + ((e & f) ^ (~e & g)) + k[i] + w[i];
		uint32_t t2 = (rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22)) + ((a & b) ^ (a & c) ^ (b & c));
		h_ = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	h[0] += a;
	h[1] += b;
	h[2] += c;
	h[3] += d;
	h[4] += e;
	h[5] += f;
	h[6] += g;
	h[7] += h_;
}

/*===============================================================================*/
///
///									更新処理
///
/*===============================================================================*/
void SHA256::Update(const uint8_t* data, size_t length) {

	while (length > 0) {
		size_t chunkSize = std::min(length, BlockSize_ - bufferSize_);
		std::memcpy(buffer_.data() + bufferSize_, data, chunkSize);
		bufferSize_ += chunkSize;
		data += chunkSize;
		length -= chunkSize;
		bitLength_ += chunkSize * 8;

		if (bufferSize_ == BlockSize_) {
			Transform(buffer_.data());
			bufferSize_ = 0;
		}
	}
}

/*===============================================================================*/
///
///								SHA-256ハッシュ計算
///
/*===============================================================================*/
std::array<uint8_t, 32> SHA256::Digest() {

	static constexpr uint8_t pad[BlockSize_] = { 0x80 };
	uint8_t bitLenBuffer[8];

	for (int i = 0; i < 8; ++i) {
		bitLenBuffer[7 - i] = bitLength_ >> (i * 8);
	}

	Update(pad, 1 + (BlockSize_ - 9 - (bitLength_ % BlockSize_ + 1) % BlockSize_));
	Update(bitLenBuffer, 8);

	std::array<uint8_t, 32> result;
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 4; ++j) {
			result[i * 4 + j] = h[i] >> (24 - j * 8);
		}
	}

	return result;
}
