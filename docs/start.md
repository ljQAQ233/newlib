# start

这一节说, 配置 与 简单的编译, 没有任何自定义的代码片段

## 交叉编译器

像其他的移植教程类似, 我们先设置 交叉编译器

对于 `x86_64` 目标的平台, 这里直接使用宿主机的 `gcc` / `binutils` 套件

但是 `newlib` 移植要求提供一个 `triplet`, 当然不嫌麻烦, 可以直接设置 `CC`, `AS` 等环境变量

对于 名为 `textos` 的系统:

### shell

```shell
mkdir -p bin
```

软链接:

```shell
for t in gcc cc g++ ar as ld ranlib nm objcopy objdump strip readelf; do
  ln -sf "$(command which $t)" bin/x86_64-unknown-textos-$t
done
```

---

设置环境变量:

```shell
export PATH=$PATH:$PWD/bin
```

### nix

```shell
$ nix develop
```

[toolchain.nix](https://github.com/ljQAQ233/newlib/blob/textos-4.6.0/nix/toolchain.nix)

## 项目配置

这是一个 **GNU autotools** 构建的项目, 一般特征是 `configure` 脚本

---

查看编译帮助:

```shell
./configure --help
```

跑一下配置, 这里安装到 `/tmp/inst` 下:

```shell
./configure --target=x86_64-unknown-textos --prefix=/tmp/inst
```

## 编译试水

```shell
make -j$(nproc)
make install
```

---

在安装目录下面可以看到:

```shell
$ ls /tmp/inst
x86_64-unknown-textos
```

```shell
$ tree /tmp/inst
/tmp/inst/x86_64-unknown-textos
├── include
│   ├── alloca.h
│   ├── _ansi.h
│   ├── argz.h
│   └── ...
└── lib
    ├── libc.a
    ├── libg.a
    ├── libm.a
    ├── libnosys.a
    └── nosys.specs

8 directories, 75 files
```
