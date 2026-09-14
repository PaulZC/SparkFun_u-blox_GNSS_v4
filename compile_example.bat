::Uncomment "docker builder prune -f" below to clear the build cache
::docker builder prune -f
docker build -t example1_positionvelocitytime --no-cache-filter deployment .
docker create --name=example1 example1_positionvelocitytime:latest
docker cp example1:/Example1_PositionVelocityTime.ino.bin .
docker cp example1:/Example1_PositionVelocityTime.ino.elf .
docker cp example1:/Example1_PositionVelocityTime.ino.bootloader.bin .
docker cp example1:/Example1_PositionVelocityTime.ino.partitions.bin .
docker container rm example1
