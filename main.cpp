
// ↓ Novice ↓
#include <Novice.h>
/*------------------------------------------------------------*/

// ↓　OpenCV ↓
#include "OpenCV.h"

const char kWindowTitle[] = "LE2B_26_ムラタ_サクヤ_TR1_1";

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// Noviceの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	/*-----------------------------------------------------------------------------------------------------------*/
	// ウィンドウに出力する値、赤いターゲットの中心座標
	// centerPos
	Vector2d centerPos;

	/*-----------------------------------------------------------------------------------------------------------*/
	// OpenCVの初期化

	OpenCV openCV;

	// 初期化
	openCV.Initialize();

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



		/*-----------------------------------------------------------------------------------------------------------*/
		// 更新処理



		// openCVの更新処理
		openCV.Update();

		// openCVから中心座標の代入
		centerPos.x = openCV.GetCenterPos().x;
		centerPos.y = openCV.GetCenterPos().x;



		/*-----------------------------------------------------------------------------------------------------------*/
		// 描画処理



		// openCVの描画処理
		openCV.Draw();



		/*-----------------------------------------------------------------------------------------------------------*/
		// デバッグ表示

		// カメラから取得した中心座標の表示
		Novice::ScreenPrintf(24, 24, "centerPosX = %d", centerPos.x);
		Novice::ScreenPrintf(24, 48, "centerPosY = %d", centerPos.y);

		// フレームの終了
		Novice::EndFrame();

		// escキーでループを抜ける
		if ((preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0)) {
			break;
		}
	}

	// 解放処理
	openCV.Finalize();
	Novice::Finalize();

	return 0;
}