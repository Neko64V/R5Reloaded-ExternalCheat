## 概要
しばし前に書き直したシンプルなR5Reloadedのチートです。  
開発者数名とも、R5Reloadedに興味を失ったので公開しておきます。

## 仕様
「誰でも簡単・安全に」をモットーに開発していました。  
メニューにImGuiを採用、ESP等のレンダリングもImGuiのDrawListを使用しています。  
また、ImGuiに最低限のカスタムが施されているので参考にするのもいいでしょう。（ImGui/Custom.cpp）
(肥大化しましたが) ある程度フレームワーク化されているので、他のゲームのチートへ比較的簡単に転用可能です。  
なお、OpenProcessを採用しているのでEACやBEのようなアンチチートには太刀打ちできません。

## 実行
必要なライブラリは全て揃っているので、VisualStudioでビルドすれば普通に動くはずです。  
https://learn.microsoft.com/ja-jp/cpp/build/vscpp-step-0-installation?view=msvc-170

## 免責事項
このプロジェクトは学習や研究・教育用としてアップロードされました。  
これらの用途以外で使用した場合に発生した如何なる損害等についても、製作者(Neko64V)は一切の責任を負いません。

## 使用したライブラリ
・Microsoft DirectXTK->SimpleMath  
https://github.com/microsoft/DirectXTK
・ImGui  
https://github.com/ocornut/imgui
・FreeType  
https://github.com/freetype/freetype

## 写真
![AIM](https://github.com/user-attachments/assets/37da052f-b8b1-4072-b6a2-c7bbd2fd3647)
