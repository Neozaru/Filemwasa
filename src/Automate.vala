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
				
				print("Changed FROM state '"+current_state.label+"'' TO state '"+trans.etat_cible.label+"'\n");
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

	public void print_current_state() {

		print("# Current state : '"+current_state.label+"'\n");
		print("# Transitions : \n");
		foreach( Transition trans in current_state.transitions ) {
			print("### "+trans.label+" ["+trans.garde+"]\n");
		}

	}

	/* Quand l'user demande une reception de message quelconque
... on verifie tout de meme si l'une des transitions comprend
... une reception de message */
	public bool should_receive() {

		foreach( Transition trans in current_state.transitions ) {

			if ( trans.garde[0] == '?' ) {
				return true;
			}

		}

		return false;

	}


	public void on_command( string[] cmd ) {

		if ( cmd.length > 0 ) {

			if ( cmd[0][0] == '!' ) {

				if ( this.check_garde(cmd[0]) ) {

					string msg = cmd[0].replace("!","");
					send_message(msg);
					print("Sent message '"+msg+"'\n");
					this.on_garde(cmd[0]);
				}
				else {
					print("/!\\ Unsupported action for current state\n");
				}
			}
			else if ( cmd[0][0] == '?' ) {

				if ( !should_receive() ) {
					print("Shouldn't receive anything now\n");
					return;
				}

				string msg = "";
				while ( msg.length < 1 ) {
					print("Waiting for a message\n");
					msg = recv_message();
				}
					
				print("Received message '"+msg+"'\n");

				if ( this.check_garde("?"+msg) ) {
					this.on_garde("?"+msg);
				}
				else {
					print("Unexpected message\n");
				}
				
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