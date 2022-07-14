f = open("./example/test.hl", "w")

for i in range(0, 100):
    f.write(f"var t{i}: i32 = 10;\n")

f.close()
