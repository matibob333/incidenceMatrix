import simple_graphs


if __name__ == '__main__':
    # g = simple_graphs.IncidenceMatrix("ICRdlpzxo")
    # print(simple_graphs.IncidenceMatrix.vertices())
    # g = simple_graphs.IncidenceMatrix("@")
    g = simple_graphs.IncidenceMatrix.create_path(3)
    print(g.vertices())
    print(g.edges())


