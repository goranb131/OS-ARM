This project uses the LLVM/Clang toolchain on macOS:

<pre>
Homebrew clang version 20.1.7  
Target: arm64-apple-darwin24.5.0  
Thread model: posix  
InstalledDir: /opt/homebrew/Cellar/llvm/20.1.7/bin  
Configuration file: /opt/homebrew/etc/clang/arm64-apple-darwin24.cfg
</pre>

At this stage of development, the project files are not organized into traditional directory structure (like include/, src/, etc). Everything is in a single flat directory for now. 

Build using *make*. It builds both kernel and separate flat binary user shell, using the LLVM/Clang cross-toolchain for ARM64 (aarch64-elf).

To run the OS in QEMU: *qemu-system-aarch64 -M virt -cpu cortex-a72 -m 2048 -serial mon:stdio -kernel kernel.elf*
