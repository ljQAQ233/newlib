# 如何编译

这一篇讲怎么把这个仓库编译出来. 构建一律在源码树之外进行, 也就是另建一个 build 目录, 不要在源码目录里跑 `make`.

## 前置条件

编译 newlib 需要一个能生成目标代码的 C 编译器和 binutils. 常见做法是先有一套交叉工具链 (比如 `arm-none-eabi-gcc`), 或者用 `--with-cross-bootstrap` 让 newlib 用宿主工具链自举.

编译 Cygwin 还需要:

- mingw-w64 交叉工具链 (除非用 `--with-cross-bootstrap`)
- `docbook2texi` 和 `xmlto` (生成手册页, 或用 `--disable-doc` 跳过)

如果是从 git 检出的源码, 仓库里可能没有生成好的 `configure`. 先补齐:

- newlib: 在 `newlib/` 里跑 `autoreconf` (autoconf 2.69 + automake 1.15.1)
- Cygwin: 在 `winsup/` 里跑 `./autogen.sh`

生成物 newlib 会提交进 git, Cygwin 不会, 细节见 `docs/buildsys.md`.

## 基本步骤

```
mkdir build && cd build
../configure --target=<triple> --prefix=$PWD/install
make
make install
```

`--target` 决定为哪个三元组构建, `--prefix` 决定安装位置. 默认 silent rules, 只打印简短进度; 想看真实编译命令用 `make V=1`, 想并行编译用 `make -j$(nproc)`.

## 常用选项

- `--target=<triple>` - 目标三元组, 决定编译哪套库
- `--prefix=<dir>` - 安装前缀
- `--disable-doc` - 不生成文档, 省掉 docbook/xmlto 依赖
- `--with-cross-bootstrap` - 用宿主工具链自举, 不需要预先的交叉工具链
- `--enable-multilib` / `--disable-multilib` - 是否为每个 ABI 变体各构建一份 (newlib 默认开启)

## 具体例子

### 嵌入式 newlib

以一个裸机目标为例, 需要预先有该目标的 gcc/binutils:

```
mkdir build && cd build
../configure --target=arm-none-eabi --prefix=$PWD/install
make
make install
```

产物是 `install/arm-none-eabi/lib/` 下的 `libc.a`, `libm.a` 和配套头文件.

### Cygwin

Cygwin 只支持 `x86_64-pc-cygwin`:

```
mkdir build && cd build
../configure --target=x86_64-pc-cygwin --prefix=$PWD/install --disable-doc
make
make install
```

产物主要是 `x86_64-pc-cygwin/winsup/cygwin/cygwin1.dll` 和对应的导入库.

### textos (本地特例)

这个仓库本地支持一个 `x86_64-unknown-textos` 目标, 它不需要交叉工具链, 只要把宿主 gcc/binutils 软链到目标前缀即可:

```
for t in gcc cc g++ ar as ld ranlib nm objcopy objdump strip readelf; do
  ln -sf "$(which $t)" /path/to/tc/bin/x86_64-unknown-textos-$t
done
PATH=/path/to/tc/bin:$PATH ../configure --target=x86_64-unknown-textos --prefix=$PWD/install
make
```

注意这个目标的 `configure` / `Makefile.in` 没有提交, 要先在 `newlib/` 里 `autoreconf`.

## 测试

newlib 的测试在顶层 build 目录里跑:

```
make check-target-newlib
```

需要 `DEJAGNU` 指向一个定义了 `target_list` 的 `site.exp`, 细节见 `newlib/README`.

Cygwin 的测试只能在 Windows 上原生运行 (不支持交叉测试), 且需要 `cygserver`:

```
cd <build>/x86_64-pc-cygwin/winsup
make check
```

只编译测试程序不运行用 `make -C winsup/testsuite check_programs`, 挑单个测试用 `make check TESTS="winsup.api/ltp/umask03"`. 测试退出码 0 为通过, 77 为跳过.

---

本文档由 deepseek-v4-pro 撰写.
