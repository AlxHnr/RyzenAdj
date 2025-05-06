/* Uses the sysfs interface provided by the ryzen_smu kernel module instead of /dev/mem and libpci. */

#include "nb_smu_ops.h"

#include "libsmu.h"

struct libsmu_handle {
	smu_obj_t smu_obj;
};

static void warn_libsmu_return_val(const smu_return_val return_val)
{
  printf("ryzen_smu module returned: %s\n", smu_return_to_str(return_val));
}

pci_obj_t init_pci_obj(void){
	libsmu_handle_t *result = malloc(sizeof *result);
	if(result == NULL){
		return NULL;
	}
	if(smu_init(&result->smu_obj) != SMU_Return_OK){
		free(result);
		return NULL;
	}
	return result;
}

void free_pci_obj(pci_obj_t obj){
	smu_free(&obj->smu_obj);
	free(obj);
}

nb_t get_nb(pci_obj_t obj){
	return obj;
}

void free_nb(nb_t nb){
	(void)nb;
}

mem_obj_t init_mem_obj(const uintptr_t physAddr)
{
	(void)physAddr;
	return (mem_obj_t)0xDEADC0DE; /* Hack to be compatible with the API in nb_smu_ops.h. */
}

void free_mem_obj(mem_obj_t obj)
{
	(void)obj;
}

u32 smn_reg_read(nb_t nb, const u32 addr)
{
	u32 result = 0;
	const smu_return_val return_val = smu_read_smn_addr(&nb->smu_obj, addr, &result);
	if(return_val != SMU_Return_OK){
		warn_libsmu_return_val(return_val);
		return 0;
	}
	return result;
}

void smn_reg_write(nb_t nb, const u32 addr, const u32 data)
{
	const smu_return_val return_val = smu_write_smn_addr(&nb->smu_obj, addr, data);
	if(return_val != SMU_Return_OK){
		warn_libsmu_return_val(return_val);
	}
}

int copy_pm_table(nb_t nb, void *buffer, const size_t size)
{
	const smu_return_val return_val = smu_read_pm_table(&nb->smu_obj, buffer, size);
	if(return_val != SMU_Return_OK){
		warn_libsmu_return_val(return_val);
		return -1;
	}
	return 0;
}

int compare_pm_table(void *buffer, const size_t size)
{
	(void)buffer;
	(void)size;
	printf("internal error: compare_pm_table() should never be called if smu driver is available\n");
	return -1;
}

bool is_using_smu_driver(void)
{
	return true;
}
