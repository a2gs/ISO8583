/*
 * Propriety of André Augusto Giannotti Scotá
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs/
 *
 * Creative Commons Public Licenses 4.0 CC BY-NC-ND
 */

#ifndef __ISO8583_PLUGINS_H__
#define __ISO8583_PLUGINS_H__

typedef struct _fdLib_t{
	void *fd;
	struct _fdLib_t *next;
}fdLib_t;

int loadPlugins(fdLib_t **pluginsList, char *pathPlugins);
int unloadPlugins(fdLib_t *pluginsList);

#endif
