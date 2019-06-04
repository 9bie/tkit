# HOOK技术篇
详解各种奇淫绝技。

部分手写部分搬运。

重在自己的理解，或者找个时间，武器化这部分程序。

最终目标： 实现一个带有各种HOOK的汇总工具包


# 遇到的坑
pragma comment 只对微软编译器有效，GCC没用，得用-l来连接

## IAT HOOK
### 内存版本
这个需要进入到宿主文件的内存空间，所以一般用DLL注入到宿主文件

原理就是普通的计算*IMAGE_IMPORT_DESCRIPTOR*在内存空间的地址。

然后修改*IMAGE_IMPORT_DESCRIPTOR.FirstThunk.u1.Function*的地址为我们的函数地址

![img][3]

其中几个重点：
```
GetModuleHandle(NULL);
// 获取主模块（EXE）的地址

PVOID WINAPI ImageDirectoryEntryToData( 
__in   PVOID Base, 
__in   BOOLEAN MappedAsImage, 
__in   USHORT DirectoryEntry, 
__out  PULONG Size );

获取内存中IAT导入表的虚拟地址。

//Base为要获得导入段所在模块的基地址。它一般是GetModuleHandle的返回值。

//MappedAsImage，它为true时，系统将该模块以映像文件的形式映射，否则以数据文件的形式映射。

//DirectoryEntry，要获得的段的索引。此函数不仅仅能够获得导入段的地址，根据此索引的不同，该函数返回对应段的地址。此处我们使用 IMAGE_DIRECTORY_ENTRY_IMPORT表示我们要获得导入段的地址。

//Size返回表项的大小

```
#### 目录文件

 - iat/memory/target.c 实例，直接封装在代码中的HOOK，没使用装载器
 - iat/memory/dll.c dll封装版本（纯搬运）

代码参考：

  - [一篇文章带你理解HOOK技术][1]
  - [IAT HOOK] [2]

### 文件区块修改版本

直接操作PE文件，修改IAT导入表导入DLL的地址。

这样的好处是在程序最初的加载就已经执行我们的代码，

并且不需要使用注入等容易被检测的敏感动作。

缺点是不能针对单独函数hook，

或者说与其是hook。完全是无中生有添加了一个DLL引用。

![pe][4]

未完待续。。。


  [1]: https://bbs.pediy.com/thread-251412.htm
  [2]: https://www.cnblogs.com/dliv3/p/6398960.html
  [3]: img/IMAGE_IMPORT_DESCRIPTOR.gif
  [4]: img/pe.bmp