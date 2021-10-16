set shell := ["bash", "-c"]

uni := "ks3343"
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

make *args: pre-make
    cd user/module/supermom && make -j{{n_proc}} {{args}}

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

compile-commands: (make "clean")
    bear -- just make
    command -v ccache > /dev/null \
        && sd "$(which ccache)" "$(which gcc)" compile_commands.json \
        || true

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
