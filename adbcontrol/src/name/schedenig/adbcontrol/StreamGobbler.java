/*******************************************************************************
 * Copyright (c) 2014 Marian Schedenig
 * 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Marian Schedenig - initial API and implementation
 *******************************************************************************/
package name.schedenig.adbcontrol;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class StreamGobbler extends Thread
{
	private InputStream in;
	private OutputStream out;
	private boolean removeCRLF;

	public StreamGobbler(InputStream in, OutputStream out, boolean removeCRLF)
	{
		this.in = in;
		this.out = out;
		this.removeCRLF = removeCRLF;
	}

	public StreamGobbler(InputStream in, OutputStream out)
	{
		this(in, out, false);
	}

	@Override
	public void run()
	{
		byte[] buffer = new byte[8192];
		OutputStream temp_out = out;
		if( removeCRLF )
			temp_out = new ByteArrayOutputStream();

		try
		{
			for(;;)
			{
				if(Thread.interrupted())
				{
					break;
				}

				int count = in.read(buffer);

				if(count < 0)
				{
					break;
				}

				if(count > 0 && out != null)
				{
					temp_out.write(buffer, 0, count);
				}
			}
			if( this.removeCRLF ) {
				byte[] temp_array = ((ByteArrayOutputStream)temp_out).toByteArray();
				for (int i = 0; i < temp_array.length; ++i) {
					if (temp_array[i] != '\r' || i + 1 >= temp_array.length || temp_array[i + 1] != '\n' ) {
						out.write(temp_array[i]);
					}
				}
			}
		}
		catch(IOException ex)
		{
			ex.printStackTrace();
		}
	}
}
