#pragma once
// メモ、TODO、その他もろもろ何でも書き留めておくファイル

// std::vector-> https://cpprefjp.github.io/reference/vector/vector/size.html
// std::for_each-> https://cpprefjp.github.io/reference/algorithm/for_each.html
//enumerate = (hid_device_info)Marshal.PtrToStructure(ptr, typeof(hid_device_info));		// [?]c++の値をc#の仕様に変換する


// static const unsigned short vendor_id = 0x57e;
// JoyconManager.hでvendor_idを初期化していないのにビルドが通るのがおかしい

// jopyconのpoll実行確認

// namespace内で宣言された同じ名前のstatic変数があるからダメ説 -> 前方宣言もネームスペースの中で

// float JimaXengine::General::ConvertToDegrees(float& radians)参照渡しにするとx64環境だと64bitになって逆に容量食う

// Todo デッドゾーン、上限、下限、の設定

// Joycon.cpp 545行ローカル配列をリターンしているのを直す