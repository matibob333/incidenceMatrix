import simple_graphs


if __name__ == '__main__':
    g = simple_graphs.IncidenceMatrix("ICRdlpzxo")
    # g = simple_graphs.IncidenceMatrix("@")
    print(g.number_of_vertices())
    print(g.vertices())
    print(g.matrix)
    print(g.edges())
