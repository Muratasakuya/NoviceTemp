#pragma once

// ↓ std ↓
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <array>
#include <cstring>
#include <bitset>

/*========================================================*/
/// SHA-256 class
/*========================================================*/
class SHA256{
public:
	/*----------------*/
	/// メンバ関数
	/*----------------*/

	// コンストラクタ
	SHA256();

	// 更新
	void Update(const uint8_t* data, size_t length);

	std::array<uint8_t, 32> Digest();

private:
	/*----------------*/
	/// メンバ変数
	/*----------------*/

	void Transform(const uint8_t* chunk);

	static const size_t BlockSize_ = 64;

	std::array<uint32_t, 8> h;
	std::array<uint8_t, BlockSize_> buffer_;
	size_t bufferSize_;
	uint64_t bitLength_;
};