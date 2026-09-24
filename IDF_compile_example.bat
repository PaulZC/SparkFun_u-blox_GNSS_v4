if [%1]==[] goto useDefault

set EXAMPLE=%1
goto dockerBuild

:useDefault

set EXAMPLE=PollingExample1_PositionVelocityTime

:dockerBuild

::Uncomment "docker builder prune -f" below to clear the build cache
::docker builder prune -f
docker build -f IDF.Dockerfile -t idf_example_container --progress=plain --no-cache-filter deployment^
 --build-arg EXAMPLE=%EXAMPLE% .
docker create --name=idf_example idf_example_container:latest
docker cp idf_example:/%EXAMPLE%.bin IDF_%EXAMPLE%.bin
docker cp idf_example:/bootloader.bin IDF_%EXAMPLE%.bootloader.bin
docker cp idf_example:/partition-table.bin IDF_%EXAMPLE%.partition-table.bin
docker container rm idf_example
