# -*- mode: ruby -*-

Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/bionic64"
  config.vm.box_check_update = false
  config.vm.synced_folder ".", "/home/vagrant/src"
  config.vm.provision "shell", inline: <<-SHELL
    apt-get update
    apt-get install -y build-essential gdb git gcc-multilib nasm
    apt-get install -y libsdl1.2-dev libtool-bin libglib2.0-dev libz-dev libpixman-1-dev

    bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"
    apt-get install -y clang
    apt-get install -y qemu-system-i386

    cat << EOF
add-auto-load-safe-path /home/vagrant/src/xv6/.gdbinit
add-auto-load-safe-path /home/vagrant/src/c_version/.gdbinit'
EOF > /home/vagrant/.gdbinit
  SHELL
end
