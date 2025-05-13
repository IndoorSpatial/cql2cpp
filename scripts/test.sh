./cql2 parse "NOT TRUE OR NOT TRUE AND FALSE" -O "test" && dot -Tpng test.dot -o test.png && eog test.png
./cql2 evaluate --features=../test/geojson/1.geojson --index 0 -O test "S_INTERSECTS  (geom,   POLYGON((-64 32, -65 18, -80 25, -64 32)))" && dot -Tpng test.dot -o test.png
./cql2 filter --features=../test/geojson/1.geojson "S_INTERSECTS  (geom,   POLYGON((-1 -1, 1 -1, 1 1, -1 1, -1 -1)))" && dot -Tpng test.dot -o test.png && eog test.png
