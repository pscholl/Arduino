/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  DfuUploader - uploader implementation using dfu-programmer

  Copyright (c) 2012
  Philipp M. Scholl

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  $Id$
*/

package processing.app.debug;

import processing.app.Base;
import processing.app.Preferences;
import processing.app.Serial;
import processing.app.SerialException;
import processing.app.SerialNotFoundException;
import static processing.app.I18n._;

import java.io.*;
import java.util.*;
import java.util.zip.*;
import javax.swing.*;
import gnu.io.*;


public class DfuUploader extends Uploader  {
  public DfuUploader() {
  }

  public boolean uploadUsingPreferences(String buildPath, String className, boolean usingProgrammer)
  throws RunnerException, SerialException {
    List erase_cmd = new ArrayList(),
         flash_cmd = new ArrayList(),
         start_cmd = new ArrayList();

    erase_cmd.add("dfu-programmer");
    flash_cmd.add("dfu-programmer");
    start_cmd.add("dfu-programmer");

    String mcu = Base.getBoardPreferences().get("build.mcu");
    erase_cmd.add(mcu);
    flash_cmd.add(mcu);
    start_cmd.add(mcu);

    erase_cmd.add("erase");
    flash_cmd.add("flash");
    start_cmd.add("start");

    if (Preferences.getBoolean("upload.verbose")) {
      erase_cmd.add("--debug");
      erase_cmd.add("25");
      flash_cmd.add("--debug");
      flash_cmd.add("25");
      start_cmd.add("--debug");
      start_cmd.add("25");
    }

    flash_cmd.add(buildPath + File.separator + className + ".hex");

    // start the reset cycle as implemented in Leonardo, we don't wait for a
    // serial port to enumerate but on the Atmel DFU device.
    String uploadPort = Preferences.get("serial.port");

    // need to do a little dance for Leonardo and derivatives:
    // open then close the port at the magic baudrate (usually 1200 bps) first
    // to signal to the sketch that it should reset into bootloader. after doing
    // this wait a moment for the bootloader to enumerate. On Windows, also must
    // deal with the fact that the COM port number changes from bootloader to
    // sketch.
    // Toggle 1200 bps on selected serial port to force board reset.
    System.out.println("Forcing reset using 1200bps open/close on port "
                    + uploadPort);
    Serial.touchPort(uploadPort, 1200);

    // try a few times until the atmel got enumerated
    int TRIES = 30, i=0;

    for (i=0; i<TRIES && !executeUploadCommand(erase_cmd); i++)
      try { Thread.sleep(100); }
      catch (InterruptedException e) {
        throw new RunnerException(e.getMessage());
      }

    if (i==TRIES)
      throw new RunnerException("could not put device into programming mode.");

    if (!executeUploadCommand(erase_cmd)) return false;
    if (!executeUploadCommand(flash_cmd)) return false;
    boolean result = executeUploadCommand(start_cmd);

    /* fix re-open to dfu-mode bug */
    while (!Serial.touchPort(uploadPort,9600))
      ;

    for (i=0; i<TRIES && !executeUploadCommand(start_cmd); i++)
      try { Thread.sleep(100); }
      catch (InterruptedException e) {
        throw new RunnerException(e.getMessage());
      }

    if (i==TRIES)
      throw new RunnerException("could not put device into programming mode.");

    return result;
  }

  public boolean burnBootloader()
    throws RunnerException {
    throw new RunnerException("bootloader is not programmable (but should be there anyhow");
  }
}
