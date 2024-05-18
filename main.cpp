
// ↓ Novice ↓
#include <Novice.h>
/*------------------------------------------------------------*/

// ↓　OpenCV ↓
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
/*------------------------------------------------------------*/

// ↓ std ↓
#include <string>
#include <iostream>
/*------------------------------------------------------------*/

// cvの省略
using namespace cv;
/*------------------------------------------------------------*/

const char kWindowTitle[] = "LE2B_26_ムラタ_サクヤ_TR1_1";

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// Noviceの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	/*-----------------------------------------------------------------------------------------------------------*/
	// ウィンドウに出力する値、赤いターゲットの中心座標
	// centerPos
	int centerPosX = 0;
	int centerPosY = 0;

	/*-----------------------------------------------------------------------------------------------------------*/
	// OpenCVの初期化

	// ウィンドウの名前
	cv::namedWindow("OpenCV Window", cv::WINDOW_AUTOSIZE);

	// カメラキャプチャ
	cv::VideoCapture cap(0);
	if (!cap.isOpened()) {
		std::cerr << "Failed to open camera!" << std::endl;
		return -1;
	}

	// カメラウィンドウのサイズ
	cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);  // 横幅
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 360); // 縦幅
	cap.set(cv::CAP_PROP_FPS, 60);           // フレームレート (FPS)

	// OpenCVで画像を表すための基本的なデータ型
	cv::Mat
		frame,    // カメラからキャプチャしたフレーム(画像データ)の保持
		hsvFrame, // BGR色空間からHSV色空間に変換したフレームの保持
		mask1,    // 赤色の第1範囲に該当するピクセルを抽出するためのマスク(バイナリ画像)の保持
		mask2,    // 赤色の第2範囲に該当するピクセルを抽出するためのマスク(バイナリ画像)の保持
		mask;     // mask1とmask2を論理和(OR)した結果を保持するマスク(バイナリ画像)の保持

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// 
	///									↓ メインループ ↓
	///
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	while (Novice::ProcessMessage() == 0) {

		// フレームの開始
		Novice::BeginFrame();

		// Input、今は使っていない
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		// カメラから取得した中心座標の表示
		Novice::ScreenPrintf(24, 24, "centerPosX = %d", centerPosX);
		Novice::ScreenPrintf(24, 48, "centerPosY = %d", centerPosY);

		/*-----------------------------------------------------------------------------------------------------------*/
		// ↓ カメラ読み取り処理

		// ウェブカメラからの画像データの読み込み
		cap >> frame;

		// 取得したフレームの左右反転
		//  第3引数 : 0 = 垂直方向方向 || 1 = 水平方向
		// 今回は鏡のように反転させるため水平方向に反転させる zoomと同じ (原理は知らない)
		cv::flip(frame, frame, 1);

		// 平滑化処理 (ガウシアンブラー)
		// 第3引数 : カーネルサイズ (ぼかしの範囲) 必ず奇数
		// 第4引数 : σ (シグマ、標準偏差) シグマが大きいほどぼかしは強い
		cv::GaussianBlur(frame, frame, cv::Size(5, 5), 0);

		// BGRからHSVへ変換
		// BGR : Blue Green Red || HSV : Hue(色相) Saturation(彩度) Value(明度)
		// 変換元の入力画像から変換後の出力画像に変換
		cv::cvtColor(frame, hsvFrame, COLOR_BGR2HSV);

		// 赤色の範囲をマスクする(2つの範囲を設定)
		// 色相下限
		cv::Scalar lowerRed1(0, 120, 70);
		// 色相上限
		cv::Scalar upperRed1(10, 255, 255);
		// 設定したマスク範囲の出力
		cv::inRange(hsvFrame, lowerRed1, upperRed1, mask1);

		// 色相下限
		cv::Scalar lowerRed2(170, 120, 70);
		// 色相上限
		cv::Scalar upperRed2(180, 255, 255);
		// 設定したマスク範囲の出力
		cv::inRange(hsvFrame, lowerRed2, upperRed2, mask2);

		// 2つのマスクを結合
		cv::bitwise_or(mask1, mask2, mask);

		// ノイズ除去のためにマスクを処理
		cv::erode(mask, mask, cv::Mat(), cv::Point(-1, -1), 2);
		cv::dilate(mask, mask, cv::Mat(), cv::Point(-1, -1), 2);

		// 赤い場所の輪郭部分を検出
		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(mask, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

		// 最大の輪郭を見つける
		// 最大の輪郭のインデックス、-1は見つかっていない状態
		int largestContourIndex = -1;
		// 最大面積
		double largestArea = 0;
		for (size_t i = 0; i < contours.size(); i++) {
			double area = cv::contourArea(contours[i]);
			if (area > largestArea) {
				largestArea = area;
				largestContourIndex = static_cast<int>(i);
			}
		}

		// 最大の輪郭が見つかった場合、その重心を計算
		if (largestContourIndex != -1) {

			// 輪郭のモーメント計算、モーメントは、形状の中心や分布を示す統計量
			cv::Moments moments = cv::moments(contours[largestContourIndex]);

			// 中心座標の計算
			int centerX = static_cast<int>(moments.m10 / moments.m00);
			int centerY = static_cast<int>(moments.m01 / moments.m00);

			// 計算した中心座標を基に円を描画
			cv::circle(frame, cv::Point(centerX, centerY), 8, cv::Scalar(0, 255, 0), -1);
			std::cout << "Center: (" << centerX << ", " << centerY << ")" << std::endl;

			// 出力する値に代入
			centerPosX = centerX;
			centerPosY = centerY;
		}

		// 画像(今回はカメラのフレーム)、ウィンドウの表示
		cv::imshow("OpenCV Window", frame);

		// フレームの終了
		Novice::EndFrame();

		// escキーでループを抜ける
		if (cv::waitKey(1) == 27 || (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0)) {
			break;
		}
	}

	// 解放処理
	cap.release();
	cv::destroyAllWindows();
	Novice::Finalize();

	return 0;
}