# what 's this

这是一个壳，最初打算是依靠winrar的带密码杀软无法查杀，

和对于重复性字节会压缩的原理，制作的一款防止云上报机制。

灵感来源于压缩文件炸弹。

只要我们首次生成的木马是免杀的，然后利用再尾部添加规律性的垃圾字符增大文件

这样既不会破坏文件结构导致木马无法运行也可以让winrar压缩后恢复正常大小。

之后winrar+密码制作成压缩包，

然后我们再用另一个程序，写一个关于这个压缩包的解压缩程序。

由于解压缩只有解压缩这个动作，所以是无毒的，然后解压出来的文件大小又十分的巨大

杀软不会把文件上传到云，又因为文件时免杀的，所以杀软查杀这个大文件并不会报毒。

所以就达到了永久防云查杀的效果(理论上)

详细可以查看：[基于免杀马的永久防上报的实现以及工具](https://9bie.org/index.php/archives/223/)