public abstract class Automate {

	protected List<Etat> _etats;
	
	public List<Etat> etats {
		get { return this._etats; }
		//protected set { ajouter_etat( Etat ); }
	}

	string current_state {
		get;
		protected set;
	}


	/* Voir s'il y a un système de surchage d'operateur 
	en Vala (à l'image d'un toString / operator<<) */
	public void debug() {

		print("Debug automate\n");
		foreach ( Etat e in _etats ) {
			print("- %s\n",e.label);
			e.debug();
		}
		print("//Debug automate//\n");
	}

}