public class Transition {

	public string label;

	public Etat etat_source {
		get;
		private set;
	}

	public Etat etat_cible {
		get;
		private set;
	}

	public string garde {
		get;
		private set;
	}

	public Transition( Etat source, Etat cible, string garde, string label ) {
		this.label = label;
		this.etat_source = source;
		this.etat_cible = cible;
		this.garde = garde;
	}

	~Transition() {
		
	}

}