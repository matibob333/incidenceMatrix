import simple_graphs


if __name__ == '__main__':
    g = simple_graphs.IncidenceMatrix("ICRdlpzxo")
    # g = simple_graphs.IncidenceMatrix("@")
    print(g.number_of_vertices())
    print(g.vertices())

    g.add_vertex(12)
    print(g.number_of_vertices())
    print(g.vertices())

    print(g.number_of_vertices())
    print(g.vertices())

    print(g.number_of_edges())
    print(g.edges())
    print(g.is_edge(2, 0))
    print(g.is_edge(7, 3))
    print(g.is_edge(5, 9))
