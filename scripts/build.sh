#!/bin/bash
set -e

echo "▶ Building LearnixMiniOS..."
cd "$(dirname "$0")/.."

# Nettoyer
echo "▶ Cleaning previous build..."
make clean

# Construire
echo "▶ Building..."
make

echo "✅ Build successful!"
echo "📁 OS image: bin/os.bin"
echo "▶ To run: make run"