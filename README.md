## 概要
R5Reloadedの外部チートです。チート開発に対する興味を失ったので公開しておきます。  
近年の平均的な安価なチートとおおよそ同等の機能を備えています。研究、分析、学習や調査用にどうぞ。

## 機能
* AIMBOT
* ESP
* 観戦者リスト
* MenuKey : END

## 備考
* メニューにImGuiを採用、ESP等のレンダリングもImGuiのDrawListを使用。
* ImGuiに最低限のカスタムを施しているので参考にすることも可。（ImGui/Custom.cpp）
* ある程度フレームワーク化を行ったので、他のゲームのチートへ比較的簡単に転用可能。
* OpenProcessやTOPMOSTオーバーレイを採用しているのでEACやBEのようなアンチチートには対策が必要。

## 実行
必要なライブラリは全て揃っているので、VisualStudioでビルドすれば普通に動くはずです。  
https://learn.microsoft.com/ja-jp/cpp/build/vscpp-step-0-installation?view=msvc-170

## 免責事項
このプロジェクトは学習や研究・教育用としてアップロードされました。  
これらの用途以外で使用した場合に発生した如何なる損害等についても、製作者(Neko64V)は一切の責任を負いません。  

## 使用したライブラリ
* Microsoft DirectXTK->SimpleMath  
https://github.com/microsoft/DirectXTK  
* ImGui  
https://github.com/ocornut/imgui  
* FreeType  
https://github.com/freetype/freetype  

## 写真
![AIM](https://github.com/user-attachments/assets/37da052f-b8b1-4072-b6a2-c7bbd2fd3647)
