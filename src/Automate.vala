public abstract class Automate {

	protected List<Etat> _etats;
	
	public List<Etat> etats {
		get { return this._etats; }
	}

	Etat current_state {
		get;
		protected set;
	}

	/* Dans notre cas, "on_garde" sera appele quand un message sera envoye/recu */
	public void on_garde( string garde ) {

		foreach( Transition trans in current_state.transitions ) {
			if ( trans.garde == garde ) {
				this.current_state = trans.etat_cible;
				return;
			}
		}

	}

	/* Dans notre cas, "on_garde" sera appele quand un message sera envoye/recu */
	public bool check_garde( string garde ) {

		foreach( Transition trans in current_state.transitions ) {

			if ( trans.garde == garde ) {
				return true;
			}

		}

		return false;

	}


	public void on_command( string[] cmd ) {

		if ( cmd.length > 0 ) {
			if ( this.check_garde(cmd[0]) ) {

				if ( cmd[0][0] == '!' ) {
						string msg = cmd[0].replace("!","");
						send_message(msg);
						this.on_garde(cmd[0]);
					
				}
				else if ( cmd[0][0] == '?' ) {
					string msg = cmd[0].replace("?","");
					print("Waiting for message '"+msg+"'");
					if ( recv_message() == msg ) {
						this.on_garde(cmd[0]);
					}
				}
			}
			else {
				print("Unsupported action for current state\n");
			}

		}


	}


	public abstract void init();

	public void first_state() {

		if ( _etats.length() > 0 ) {
			this.current_state = _etats.nth_data(0);
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