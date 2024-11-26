#!/bin/bash
# (c) J~Net 2024
#
echo "Installing $1..."
echo ""
sudo cp $1 /usr/bin/
sudo chmod +x /usr/bin/$1
echo "All Done!"
echo ""
