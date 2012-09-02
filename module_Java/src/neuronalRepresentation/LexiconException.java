package neuronalRepresentation;

public class LexiconException extends Exception{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private String errorMsg="No message error specified";
	public LexiconException(String msg){
		errorMsg = msg;
	}
	
	public String toString(){
		return errorMsg;
	}
}
