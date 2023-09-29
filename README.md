# m5-atoms3u-examples


## USBMSCを使う場合のコンパイル方法とUSBSerialデバッグ方法 platformio

1. F1 > build ビルドできるか確認
2. リセットボタン長押しで離して緑色のLEDの表示確認
3. F1 > upload コンパイル書き出し
4. success後ATOMS3UをPCから抜く
5. VSCodeのterminal表示の部分がserial接続の表示に変わっているのでそれを閉じる
6. ATOMS3UをPCにUSBで接続
7. PCがストレージを認識するまで待つ
8. VSCodeのシリアルを開く
9. PCのターミナルからストレージのファイルに対してcat やecho >> をしてシリアルの表示を確認

## SPIFFSでの保存について
SPIFFSでデータを保存した場合USBをすぐに抜くのではなく、Windowsなら右クリックからの安全な取り外しをしてからUSBを抜かないと保存されない。
