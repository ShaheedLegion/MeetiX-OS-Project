/*
* MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (char *argumentat your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHout ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <eva.h>
#include <mxuser/io/files/file_log.hpp>
#include <fstream>
#include <sstream>

/*
 *	create environment file
 */
void createOsEnvironmentFile(const char *dir, const char *fileName)
{
	//open directory etc
	SetWorkingDirectory(dir);
	
	//create environment file
	std::ofstream host(fileName);

	//write in
	host << "HOSTNAME=\"MeetiX_OS\"" << std::endl;
	host << "VERSION=\"0.6.9\"" << std::endl;
	host << "PATH=\"/bin/\"" << std::endl;
	host << "THEME=\"moon\"" << std::endl;

	//closing file
	host.close();
}

/**
 *
 */
int main(int argc, char *argv[]) 
{
	// load spawner server
	RamdiskSpawnStatus spawnerStat = RamdiskSpawn("/sys/svs/spawner.sv", SECURITY_LEVEL_KERNEL);
	
	if (spawnerStat != RAMDISK_SPAWN_STATUS_SUCCESSFUL) FileLog::flog("unable to load system spawner server");

	// wait for spawner to get ready
	Tid spawnerID;
	while ((spawnerID = TaskGetID(SPAWNER_IDENTIFIER)) == -1)  Yield();

	// let the spawner load shell
	Pid mxPid;

	SpawnStatus stat = SpawnP("/bin/mx", "-s inittab.sh", "/sys/init/system", SECURITY_LEVEL_KERNEL, &mxPid);
	
	std::stringstream ss;
	if (stat == SPAWN_STATUS_SUCCESSFUL) 
	{
		ss << "mx shell executed in process " << mxPid;
		FileLog::flog(ss.str()); 
		
		FileLog::flog("Creating environment variables file");
		createOsEnvironmentFile("/etc/vars/", "env");
	}

	else
	{
		ss << "failed to load mx shell from '/bin/mx' with code " << stat;
		FileLog::flog(ss.str());
	}

}
