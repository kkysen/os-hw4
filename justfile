set shell := ["bash", "-c"]

uni := "ks3343"
hw := "4"
n_proc := `nproc`

default:
    just --list

install-program-dependencies:
    sudo apt install \
        clang-format \
        bear \
        make \
        build-essential \
        bc \
        python \
        bison \
        flex \
        libelf-dev \
        libssl-dev \
        libncurses-dev \
        dwarves
    command -v cargo > /dev/null || curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
    cargo install cargo-quickinstall
    cargo quickinstall just
    cargo quickinstall ripgrep
    cargo quickinstall fd-find
    cargo quickinstall sd
    cargo quickinstall gitui

parallel-bash-commands:
    rg '^(.*)$' --replace '$1 &' --color never
    echo wait

sequential-bash:
    bash

parallel-bash:
    just parallel-bash-commands | bash

map-lines pattern replacement:
    rg '^{{pattern}}$' --replace '{{replacement}}' --color never

pre-make:

make-in dir *args:
    cd "{{dir}}" && make -j{{n_proc}} {{args}}

make-test *args: (make-in "user/test" args)

make-mod *args: (make-in "user/module/supermom" args)

make-non-kernel *args: (make-test args) (make-mod args)

make-kernel *args: (make-in "linux" args)

make-sys-supermom *args: (make-kernel "kernel/supermom.o" args)

make: pre-make make-mod make-kernel

install: (make-kernel "modules_install") (make-kernel "install")

modified-files:
    git diff --name-only

fmt-commands *args:
    just modified-files | rg '\.(c|h)$' \
        | just map-lines '(.*)' 'clang-format {{args}} "$1"'

fmt-args *args:
    just fmt-commands {{args}} | just parallel-bash

fmt: (fmt-args "-i")

pre-commit: make fmt

gitui: pre-commit
    gitui

compile-commands-mod: (make-non-kernel "clean")
    cd user && bear -- just make-non-kernel
    command -v ccache > /dev/null \
        && sd "$(which ccache)" "$(which gcc)" user/compile_commands.json \
        || true

compile-commands-kernel:
    cd linux && ./scripts/clang-tools/gen_compile_commands.py

join-compile-commands *dirs:
    #!/usr/bin/env node
    const fsp = require("fs/promises");
    const pathLib = require("path");

    function openCompileCommands(dir) {
        const path = pathLib.join(dir, "compile_commands.json");
        return {
            async read() {
                const json = await fsp.readFile(path, "utf-8");
                return JSON.parse(json);
            },
            async write(compileCommands) {
                const json = JSON.stringify(compileCommands, null, 4);
                await fsp.writeFile(path, json);
            }
        };
    }

    async function main() {
        const dirs = "{{dirs}}".split(" ");
        const compileCommandArrays = await Promise.all(dirs.map(dir => openCompileCommands(dir).read()));
        const joinedCompileCommands = compileCommandArrays.flat();
        await openCompileCommands(".").write(joinedCompileCommands);
    }

    main().catch(e => {
        console.error(e);
        process.exit(1);
    });

compile-commands: compile-commands-mod compile-commands-kernel (join-compile-commands "user" "linux")

log *args:
    sudo dmesg --kernel --reltime {{args}}

log-watch *args: (log "--follow-new" args)

run-mod-priv dir:
    #!/usr/bin/env bash
    cd "{{dir}}"
    just log | wc -l > log.length
    #kedr start *.ko
    echo "running $(tput setaf 2){{dir}}$(tput sgr 0):"
    insmod *.ko
    rmmod *.ko
    just log --color=always | tail -n "+$(($(cat log.length) + 1))"
    rm log.length
    exit
    cd /sys/kernel/debug/kedr_leak_check
    bat --paging never info possible_leaks unallocated_frees
    kedr stop

run-mod-only dir:
    sudo env "PATH=${PATH}:/usr/local/sbin:/usr/sbin:/sbin" just run-mod-priv "{{dir}}"

run-mod dir: (run-mod-only dir)

# setup-kernel:
#     make -C linux mrproper
#     make -C linux olddefconfig
#     make -C linux menuconfig
#     ./linux/scripts/diffconfig

# build-kernel:
#     make -C linux "-j{{n_proc}}"
#     sudo make -C linux modules_install
#     sudo make -C linux install

default-branch:
    git remote show origin | rg 'HEAD branch: (.*)$' --only-matching --replace '$1'

tag name message:
    git tag -a -m "{{message}}" "{{name}}"
    git push origin "$(just default-branch)"
    git push origin "{{name}}"

submit part: (tag "hw" + hw + "p" + part + "handin" "Completed hw" + hw + " part" + part + ".")
