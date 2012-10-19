public class Etat {

	public string label {
		get;
		set;
	}

	private List<Transition> _transitions;

	public List<Transition> transitions {
		get { return _transitions; }
	}

	public void ajouter_transition( Transition t ) {
		_transitions.append(t);
	}

	public Etat( string label ) {
		this.label = label;
	}


	public void debug() {

		print("Debug etat\n");
		foreach ( Transition t in _transitions ) {
			print("%s --[%s]--> %s\n",t.etat_source.label,t.garde,t.etat_cible.label);
		}
		print("//Debug etat//\n");
	}


	~Etat() {

	}

}