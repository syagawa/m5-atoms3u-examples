# m5-atoms3u-examples

Examples for M5AtomS3U https://docs.m5stack.com/en/core/AtomS3U .

## platformioでUSBMSCを使う場合のコンパイル方法とUSBSerialデバッグ方法

1. VSCode(platformio): F1 > build ビルドできるか確認
2. リセットボタン長押しで離して緑色のLEDの表示確認
3. VSCode(platformio): F1 > upload コンパイル書き出し
4. success 後 ATOMS3UをPCから抜く
5. VSCode(platformio)のterminal表示の部分がserial接続の表示に変わっているのでそれを閉じる
6. ATOMS3UをPCに接続
7. PCがストレージを認識するまで待つ
8. VSCode(platformio): F1 > シリアルを開く
9. PCのターミナルからストレージのファイルに対してcat やecho >> をしてシリアルの表示を確認

## SPIFFSでの保存について
SPIFFSでデータを保存した場合USBをすぐに抜くのではなく、Windowsなら右クリックからの安全な取り外しをしてからUSBを抜かないと保存されない。
