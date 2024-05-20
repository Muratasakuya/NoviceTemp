#pragma once

// ↓　OpenCV ↓
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>

// ↓ std ↓
#include <string>
#include <iostream>

// ↓ Lib ↓
#include "Vector2.h"

// cvの省略
using namespace cv;

/*========================================================*/
/// OpenCV class
/*========================================================*/
class OpenCV {
private:
	/*----------------*/
	/// メンバ変数
	/*----------------*/

	// カメラキャプチャ
	cv::VideoCapture cap_;

	// OpenCVで画像を表すための基本的なデータ型
	cv::Mat
		frame_,    // カメラからキャプチャしたフレーム(画像データ)の保持
		hsvFrame_, // BGR色空間からHSV色空間に変換したフレームの保持
		mask1_,    // 赤色の第1範囲に該当するピクセルを抽出するためのマスク(バイナリ画像)の保持
		mask2_,    // 赤色の第2範囲に該当するピクセルを抽出するためのマスク(バイナリ画像)の保持
		mask_;     // mask1とmask2を論理和(OR)した結果を保持するマスク(バイナリ画像)の保持

	// 中心座標
	Vector2d centerPos_;

	// 検出できたかどうか
	bool isFound_;

	// 終了フラグ
	bool isFinish_;
	
public:
	/*----------------*/
	/// メンバ関数
	/*----------------*/

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// 描画
	void Draw();

	// 終了処理
	void Finalize();

	// getter

	// 中心座標
	Vector2d GetCenterPos() const { return centerPos_; }
};