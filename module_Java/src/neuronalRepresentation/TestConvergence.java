package neuronalRepresentation;

public class TestConvergence {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		try {
			long ttt = System.currentTimeMillis();
			Lexicon lextest = new Lexicon(20000,5000);
			long starttime = System.currentTimeMillis();
			System.out.println("Init time: "+(starttime-ttt)+"ms");
			int correct = 0;
			int correct_with_other_symbols = 0;
			int correct_after_one_more_iteration = 0;
			int correct_after_one_more_it_with_other_symbols = 0;
			int incorrect=0;
			int loopsize=100;
			for (int i=0;i<loopsize;i++){
				boolean[] x = lextest.getRandomX();
				//boolean[] u = lextest.modify_vector_random(x, 10+(i%10));
				//boolean[] u = lextest.modify_vector(x, 10+(i%20), 3);
				boolean[] u = lextest.modify_vector_2(x, 10+(i%20), 3,5,4);
				boolean[] result = lextest.execute_step_network(u,5);
				int diff = lextest.compareVector(result, x);
				if (diff!=0){
				//System.out.println("difference: "+diff+" number mod: "+(10+(i%20)));
				//System.out.println("checking with other symbols...");
				boolean match = lextest.comparetoAllVector(result);
				//System.out.println("match found? : "+match);
				if (!match){
					boolean[] r2 = lextest.execute_step_network(result, 5);
					int diff2 = lextest.compareVector(r2, x);
					if (diff2!=0){
						boolean match2 = lextest.comparetoAllVector(r2);
						if (!match2) incorrect++;
						else correct_after_one_more_it_with_other_symbols++;
					}
					else correct_after_one_more_iteration++;
				}
				else correct_with_other_symbols++;
				}
				else{
					correct++;
				}
				
			}
			System.out.println("correct : "+correct+"/"+loopsize);
			System.out.println("correct but with another symbol than the based one : "+correct_with_other_symbols+"/"+loopsize);
			System.out.println("correct after one more iteration : "+correct_after_one_more_iteration+"/"+loopsize);
			System.out.println("correct after one more iteration but with other symbols : "+correct_after_one_more_it_with_other_symbols+"/"+loopsize);
			System.out.println("incorrect : "+incorrect+"/"+loopsize);
			//lextest.writeLexState("States.bin");
			long endtime = System.currentTimeMillis();

			System.out.println("Time elapsed: "+(endtime-starttime) +"ms");

		} catch (LexiconException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.exit(1);
		}


		long total = Runtime.getRuntime().totalMemory();
		long used  = Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory();
		System.out.println("total memory usage: "+total+" bytes");
		System.out.println("memory really used: "+used+" bytes");

	}

}
