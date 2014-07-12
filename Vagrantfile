# -*- mode: ruby -*-
# vi: set ft=ruby :

# Vagrantfile API/syntax version. Don't touch unless you know what you're doing!
VAGRANTFILE_API_VERSION = "2"

Vagrant.configure(VAGRANTFILE_API_VERSION) do |config|
  # Every Vagrant virtual environment requires a box to build off of.
  config.vm.box = "ubuntu/trusty64"

$script = <<SCRIPT
DEBIAN_FRONTEND=noninteractive sudo apt-get update -q &&
DEBIAN_FRONTEND=noninteractive sudo apt-get install -yq \
  build-essential \
  g++-multilib \
  curl
SCRIPT

  config.vm.provision "shell", inline: $script

end
