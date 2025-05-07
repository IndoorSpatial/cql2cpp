./cql2 -query="NOT TRUE OR NOT TRUE AND FALSE" -dot="test" && dot -Tpng test.dot -o test.png && eog test.png
./cql2 -query="S_INTERSECTS  (geom,   POLYGON((-64 32, -65 18, -80 25, -64 32)))" -geojson=../test/geojson/1.geojson -dot="test"  && dot -Tpng test.dot -o test.png && eog test.png
