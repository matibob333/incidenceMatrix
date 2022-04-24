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

    print("\ndeleting edges")
    g.delete_edge(0, 5)
    print(g.number_of_edges())
    print(g.edges())
    g.delete_edge(9, 7)
    print(g.number_of_edges())
    print(g.edges())

    print("\nadding edges")
    g.add_edge(10, 11)
    print(g.number_of_edges())
    print(g.edges())
    g.add_edge(11, 2)
    print(g.number_of_edges())
    print(g.edges())
    g.add_edge(7, 3)
    print(g.number_of_edges())
    print(g.edges())
    g.add_edge(12, 0)
    print(g.number_of_edges())
    print(g.edges())

    print("\ndeleting vertices")
    print("before deletion")
    print(g.number_of_vertices())
    print(g.vertices())
    print(g.number_of_edges())
    print(g.edges())
    g.delete_vertex(1)
    print("after deletion")
    print(g.number_of_vertices())
    print(g.vertices())
    print(g.number_of_edges())
    print(g.edges())


