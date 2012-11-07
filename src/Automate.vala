public abstract class Automate {

	protected List<Etat> _etats;
	//protected HashMap<string, int> _maa;
	
	public List<Etat> etats {
		get { return this._etats; }
		//protected set { ajouter_etat( Etat ); }
	}

	Etat current_state {
		get;
		protected set;
	}

	/* Dans notre cas, "on_garde" sera appele quand un message sera envoye/recu */
	public void on_garde( string garde ) {

		foreach( Transition trans in current_state.transitions ) {

			if ( trans.garde == garde ) {
				this.change_state(trans);
				return;
			}

		}

	}

	/* Peut etre appele manuellement, ou par "on_garde" */
	public void change_state( Transition trans ) {

		if ( trans.etat_source == current_state ) {
			current_state = trans.etat_source;
		}

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