./cql2 -cql2_query="NOT TRUE OR NOT TRUE AND FALSE" -dot="test" && dot -Tpng test.dot -o test.png && eog test.png
