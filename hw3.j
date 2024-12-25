.source hw3.j
.class public Main
.super java/lang/Object
.method public static main([Ljava/lang/String;)V
.limit stack 100
.limit locals 100
	ldc "Hello World"
	astore 0
	aload 0
	ldc 5
	iconst_0
	swap
	invokevirtual java/lang/String/substring(II)Ljava/lang/String;
	astore 1
	aload 0
	ldc 5
	ldc 6
	invokevirtual java/lang/String/substring(II)Ljava/lang/String;
	astore 2
	aload 0
	ldc 6
	invokevirtual java/lang/String/substring(I)Ljava/lang/String;
	astore 3
	aload 1
	getstatic java/lang/System/out Ljava/io/PrintStream;
	swap
	invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
	aload 2
	getstatic java/lang/System/out Ljava/io/PrintStream;
	swap
	invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V
	aload 3
	getstatic java/lang/System/out Ljava/io/PrintStream;
	swap
	invokevirtual java/io/PrintStream/println(Ljava/lang/String;)V
	return
.end method
