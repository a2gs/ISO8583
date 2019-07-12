/*
 * Propriety of André Augusto Giannotti Scotá
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs/
 *
 * Creative Commons Public Licenses 4.0 CC BY-NC-ND
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <dlfcn.h>

#include <ISO8583.h>
#include <plugins.h>

extern bitFuncFmt_t fmtBit[];

int unloadPlugins(fdLib_t *pluginsList)
{
	fdLib_t *walker = NULL;
	fdLib_t *walkerSaved = NULL;

	for(walker = pluginsList; walker != NULL; walker = walkerSaved){
		dlclose(walker->fd);
		walkerSaved = walker->next;
		free(walker);
	}

	pluginsList = NULL;

	return(OK);
}

int addPluginLibIntoList(fdLib_t **pluginsList, char *libPath, void *lib_handle)
{
	fdLib_t *newReg = NULL, *walker = NULL;

	/* Criando ponteiro para na lista de libs para a nova lib */
	newReg = (fdLib_t *)malloc(sizeof(fdLib_t));
	if(newReg == NULL){
		fprintf(stderr, "Nao eh possivel alocar memoria para registrar lib: [%s].\n", libPath);
		return(ERRO);
	}
	newReg->fd = lib_handle;
	newReg->next = NULL;

	if(*pluginsList == NULL){
		*pluginsList = newReg;
	}else{
		/* Procurando pelo ultimo elemento da lista */
		for(walker = *pluginsList; walker != NULL; walker = walker->next){
			if(walker->next == NULL)
				break;
		}

		/* Ultimo registro apontando o novo registro (que passa a ser o ultimo) */
		walker->next = newReg;
	}

	return(OK);
}

int loadLibPluginFunctions(void *lib_handle, const pluginRegister_t *pluginList)
{
	char *errorDl = NULL;
	bitFuncFmt_t *walkerFuncFmt = NULL;

	/* Percorrendo os elementos (funcoes-plugin de formatacao) retornados na funcao de registro da lib */
	for(; pluginList->bitId != END; pluginList++){
		fprintf(stderr, "Funcoes que temos no arquivo: %s - bit [%d]\n", pluginList->funcName, pluginList->bitId);

		/* Percorrendo funcoes e alterando chamadas da lista de formatacao de bits */
		for(walkerFuncFmt = fmtBit; walkerFuncFmt->bitId != END; walkerFuncFmt++){
			if(walkerFuncFmt->bitId == pluginList->bitId){
				dlerror();
				walkerFuncFmt->func = (int (*)(bit_t *bit)) dlsym(lib_handle, pluginList->funcName);
				if((errorDl = dlerror()) != NULL){
					fprintf(stderr, "Nao eh possivel achar funcao [%s] na lib: [%s].\n", pluginList->funcName, errorDl);
					continue;
				}
			}
		}
	}

	return(OK);
}

int loadPlugins(fdLib_t **pluginsList, char *pathPlugins)
{
	void *lib_handle = NULL;
	char *errorDl = NULL;
	DIR *dir = NULL;
	struct dirent *fileEntry = NULL;
	const pluginRegister_t * (*fnReg)(void) = NULL;
	char libPath[1024] = {0};

	/* Abrindo diretorio */
	dir = opendir(pathPlugins);
	if(dir == NULL){
		fprintf(stderr, "Erro abrindo diretorio: [%s]. Motivo: [%s].\n", pathPlugins, strerror(errno));
		return(ERRO);
	}

	/* Percorrendo diretorio */
	while((fileEntry = readdir(dir)) != NULL){
		/* Validando e monstando nome da lib */
		if(strstr(fileEntry->d_name, ".so") == NULL){
			continue;
		}

		snprintf(libPath, sizeof(libPath), "%s/%s", pathPlugins, fileEntry->d_name);

		lib_handle = dlopen(libPath, RTLD_LAZY);
		if(!lib_handle){
			fprintf(stderr, "Nao eh possivel ler lib [%s]: [%s].\n", libPath, dlerror());
			continue;
		}

		/* Abrindo e lendo funcao de registro de funcoes-plugin */
		dlerror();
		fnReg = (const pluginRegister_t * (*)(void)) dlsym(lib_handle, "registerPlugin");
		if((errorDl = dlerror()) != NULL){
			fprintf(stderr, "Nao eh possivel achar funcao de registro \"registerPlugin\" no arquivo [%s]: [%s]. Descartando lib.\n", libPath, errorDl);
			continue;
		}

		/* Adicionando lib na lista */
		if(addPluginLibIntoList(pluginsList, libPath, lib_handle) == ERRO){
			fprintf(stderr, "Nao eh possivel adicionar lib [%s] na lista de libs-pluguns.\n", libPath);
			return(ERRO);
		}

		/* Carregando funcoes-plugin e substituindo na lista de formatacao */
		loadLibPluginFunctions(lib_handle, fnReg());
	}

	closedir(dir);

	return(OK);
}
