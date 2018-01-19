# -*- mode: ruby -*-
# vi: set ft=ruby :
#
# capture in host with tcpdump: tcpdump -i vboxnet1 udp -vv -X
# https://dzone.com/articles/tcpdump-learning-how-read-udp
#
# All Vagrant configuration is done below. The "2" in Vagrant.configure
# configures the configuration version (we support older styles for
# backwards compatibility). Please don't change it unless you know what
# you're doing.
Vagrant.configure("2") do |config|
  config.vm.define "compA" do |compA|
    compA.vm.box = "ubuntu/xenial64"
    compA.vm.hostname = "comp3334CompA"
    # server.vm.network "forwarded_port", guest: 22, host: 2223
    # compA.vm.network "forwarded_port", guest: 4950, host: 4950, protocol: "udp"
    compA.vm.network "private_network", ip: "192.168.30.21", name: "vboxnet2", adapter: 2
    compA.vm.synced_folder "./src/compA", "/home/ubuntu/src", mount_options: ["dmode=775,fmode=777"]
    compA.vm.provider "virtualbox" do |vb1|
      # Display the VirtualBox GUI when booting the machine
      # vb.gui = true

      # Customize the amount of memory on the VM:
      vb1.memory = "1524"
      vb1.customize ["modifyvm", :id, "--nicpromisc2", "allow-all"]
      vb1.customize ["modifyvm", :id, "--uartmode1", "disconnected"]
      # vb1.customize ["modifyvm", :id, "--nictrace1", "on"]
      # vb1.customize ["modifyvm", :id, "--nictracefile1", "file.pcap"]
    end
  end
  config.vm.define "compB" do |compB|
    compB.vm.box = "ubuntu/xenial64"
    compB.vm.hostname = "comp3334CompB"
    # client.vm.network "forwarded_port", guest: 22, host: 2224
    # compB.vm.network "forwarded_port", guest: 4950, host: 4950, protocol: "udp"
    compB.vm.network "private_network", ip: "192.168.30.22", name: "vboxnet2", adapter: 2
    compB.vm.synced_folder "./src/compB", "/home/ubuntu/src", mount_options: ["dmode=775,fmode=777"]
    compB.vm.provider "virtualbox" do |vb2|
      # Display the VirtualBox GUI when booting the machine
      # vb.gui = true

      # Customize the amount of memory on the VM:
      vb2.memory = "1524"
      vb2.customize ["modifyvm", :id, "--nicpromisc2", "allow-all"]
      vb2.customize ["modifyvm", :id, "--uartmode1", "disconnected"]
      # vb2.customize ["modifyvm", :id, "--nictrace2", "on"]
      # vb2.customize ["modifyvm", :id, "--nictracefile2", "trace2.pcap"]
    end
  end

  # View the documentation for the provider you are using for more
  # information on available options.

  # Enable provisioning with a shell script. Additional provisioners such as
  # Puppet, Chef, Ansible, Salt, and Docker are also available. Please see the
  # documentation for more information about their specific syntax and use.
  # config.vm.provision "shell", inline: <<-SHELL
  #   apt-get update
  #   apt-get install -y apache2
  # SHELL
  config.vm.provision "ansible_local" do |ansible|
    ansible.playbook = "playbook.yml"
    ansible.verbose = true
    # ansible.extra_vars = {
    #   hostname: HOSTNAME,
    #   public_ip: IP,
    #   public_mask: NETWORK_MASK,
    #   public_gateway: NETWORK_GATEWAY
    # }
    # ansible.install_mode = "default"
  end

end
