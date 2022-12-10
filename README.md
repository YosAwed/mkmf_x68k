
                 Mkmf - makefile editor (version 1.2a for X68000)

		       original (MS-DOS) Syuichi Takahashi (staka@spec.co.jp)
		       x68k Modified Awed


　　本パッケージに含まれているファイルは下記の通りです。

	mkmf.x      mkmf 本体
	makefile.p    プログラム用テンプレートファイルのサンプル(gcc用)
	makefile.l    ライブラリ用テンプレートファイルのサンプル
*	readme        変更履歴など　　（ＭＳ－ＤＯＳ用）
*	readme2       日本語マニュアル（ＭＳ－ＤＯＳ用）
	readme.x68    Ｘ６８Ｋ用の補足

	makefile　　　mkmf.x 用の makefile （gcc用)

	mkmf.h　　　　---- ソースファイル　(X68000)
	mkmf.c　　　 　|
	dir.c          |
	file.c         |
	mlib.c         |
 	suffix.c       |
	include.c      |
	argument.c　　---


　開発は Gnu make(milk.版) 
	 Gcc Ver1.37.1 X6_08 (真理子版)
	 As ver2.00 (SHARP/HUDSON)
	 hlk ver1.08(test2) (SALT版)
	 Micro emacs J1.31 (ぃかむ版)

  で行いました。これらに携わられた方に感謝の意を表します。

---------------------------- readme2.doc


                 Mkmf - makefile editor (version 1.2a for MS-DOS)

                       Syuichi Takahashi (staka@spec.co.jp)



　  Mkmf は ディレクトリから情報を集めて makefile を自動更新（作成）してく
  れるツールです。makefile 中の 下記マクロが自動更新の対象となります。また
  makefile の 末尾に '-object-: -includefile -' 形式の依存規則も追加してく
  れます。

	EXTHDRS　プログラムの使用しているシステムインクルードのリスト。
	HDRS　カレントのインクルードファイルのリスト。
	SRCS　カレントのソースファイルのリスト。
	OBJS　カレントのオブジェクトファイルのリスト。

　  Mkmf は カレントに makefile があればそれを更新しますが、無い場合は下記
  のような makefile を作成してくれます。

	（test.c と stdio.h, test.h の例）

	DEST          =
	EXTHDRS       = a:/usr/msc/include/stdio.h
	HDRS          = test.h
	SRCS          = test.c
	OBJS          = test.obj
	CC            = cl
	MAKEFILE      = makefile
	PROGRAM       = a.exe
	$(PROGRAM):	$(OBJS)
			$(CC) -o $(PROGRAM) $(OBJS)
	install:	$(PROGRAM)
			copy $(PROGRAM) $(DEST) >nul
	###
	test.obj: a:/usr/msc/include/stdio.h test.h


# Template-file
-------------------------------------------------------------------------------
　  Makefile を自分の作業環境に合わせたものにしたい場合は、環境変数 LIB で 
　指定したディレクトリに、自作したテンプレートファイルを置いてください。Mkmf
  はこれをもとに makefile を作成します。

	環境変数
		検索は USRLIB -> LIB -> MAKEPATH の順で行います。
		パスのセパレータは '/' でも OK です。

	ディフォルトのテンプレートファイル名
		プログラム用 - 'makefile.p'
		ライブラリ用 - 'makefile.l'

    尚、UNXI をまねて作成したサンプルを同封しておきましたので参考にしてくだ
  さい。


# System-include-path
-------------------------------------------------------------------------------
　   システムインクルードの検索は環境変数 INCLUDE に設定されたパスをもとに
   検索します。パスは /include;/usr/include のように ';' で 区切り複数指定
   することが可能です。（オリジナルは CFLAGS = -I に記述された inlucde-file
   も検索しますが、これは対応していません）


# Options
------------------------------------------------------------------------------
   オプションは下記の通りです。環境変数 MKMF により下記 (1) のように指定する
   こともできます。

       (1) a:\>set mkmf=-si
	   a:\>mkmf

       (2) a:\>mkmf -si


   -c   'creating makefile from...' メッセージの出力をやめます。

   -d   include-file の探索をやめます。また、makefile 内の古い依存情は修正
        されません。

   -f makefile
	ディフォルトの makefile 名を変更します。ディフォルトのファイル名は
        'makefile' です。

   -i   program 名か library 名と それを install するための directory の入
        力を行います。ディフォルトの program 名は、a.exe（library は a.lib）
        directory は current です。（template-file のもとの値）

   -l   ライブラリ用の makefile を作成します。

   -F template
        ディフォルトの templatefile 名を変更します。 ディフォルトの program
        の template は makefile.p 、library の template は、makefile.l です。

   -s   system include の検索をやめます。また、makefile にも含まれません。

   -Snn
        ファイル名を列挙する時に継続文字('\')をもちいて下記のようにします。

　　　　　  SRCS = test.c suba.c subb.c　が  SRCS = test.c \ となります。
  　      　　　　　　　　　　　　　　　　　        suba.c \
    　                                              subb.c

          また nn を指定することにより、nn byte文、ラインに詰め込んでから、
          改行します。

　　　　    SRCS = test.c \  が  SRC = test.c sub.c \ となります。
        　         suba.c \            sub.b
                   subb.c

   -e   makefile 中のパスのセパレータを '\' にします。ディフォルトは '/' です。

   -D flag...
        #ifdef～#endif の評価を行います。また CFLAGS マクロに記述されたもの
        も有効です。

   -L responsefile
        MS-linker 用の応答ファイルを作成します。

   -v   作者とバージョンを表示します。

# Examples
-------------------------------------------------------------------------------
(Ex-1)
------
    プログラム名を mkmf.exe とし、検索対象とするソースを mkmf.c と include.c
  に限定します。

	a:\>mkmf "PROGRAM=mkmf.exe SRCS=mkmf.c include.c"

(Ex-2)
------
    LDFLGS マクロを '/Stack:5000' に変更します。

	a:\>mkmf "LDFLGS=/Stack:5000"

(Ex-3)
------
　  プログラム名、インストールディレクトリパスの入力を要求し、ライブラリ用の 
  makefile を 作成します。

	a:\>mkmf -il
        library name ?
        destination directory ?

  また、-l を指定しないと、

	a:\>mkmf -il
        Proguram name ?
        destination directory ?

  となり、プログラム保守用の makefile を作成します。


(Ex-4)
------
  #ifdef(#ifndef)～#endif の評価を行い、該当の include-file のみを対象としま
  す。これは マクロに反映しません。

     a:\>mkmf -D TEST

   マクロに反映させたい場合は下記のように makefile の CFLAG を更新してください。　 
     a:\>mkmf "CFLAGS=-D TEST"

