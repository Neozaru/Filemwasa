public class Cli {
    
    //public struct

    private Automate _automate;
    private bool _running;

    static string read_stdin( bool single_line ) {

      var input = new StringBuilder ();
      var buffer = new char[1024];
      while (!stdin.eof()) {
        string read_chunk = stdin.gets (buffer);
        if (read_chunk != null) {
          input.append(read_chunk);
          if(single_line) {
            break;
          }
        }
      }
      return input.str;

    }


    public Cli( Automate auto ) {

      _automate = auto;
      //read_stdin(true);

    }

    public void start() {

      _running = true;
      while ( _running ) {

        exec_cmd();

      }

    }

    public string get_cmd() {

      string str = read_stdin(true);
      if ( str.length > 0 ) {
        return str;
      }
      else {
        return "";
      }

    }


    public void exec_cmd() {

      string cmd = get_cmd();

      if ( true ) {

        print("Command "+cmd);

      }

    }

    ~Cli() {

    }




}