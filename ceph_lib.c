#include "ceph_lib.h"

/*TODO encerrar contexto de io quando ocorrer falha de io*/

rados_t create_handle(){

	/* Initialize the cluster handle with the "ceph" cluster name and the "client.admin" user */
	
	/* Declare the cluster handle and required arguments. */
	char cluster_name[] = "ceph";
	char user_name[] = "client.admin";
	uint64_t flags;
	rados_t cluster;
	
	//create handle
	error = rados_create2(&cluster, cluster_name, user_name, flags);
	if(error < 0){
		fprintf(stderr, "[create_handle/ceph_lib.c] Couldn't create the cluster handle! %s\n", strerror(-error));
		
		exit(EXIT_FAILURE);
	}
	
	/* Read a Ceph configuration file to configure the cluster handle. */
	error =  rados_conf_read_file(cluster, "/etc/ceph/ceph.conf");
	if(error < 0){
		fprintf(stderr, "[create_handle/ceph_lib.c] Cannot read config file: %s\n", strerror(-error));
		
		exit(EXIT_FAILURE);
	}
	
	/* Connect to the cluster */
	error = rados_connect(cluster);
	if(error < 0){
		fprintf(stderr, "[create_handle/ceph_lib.c] Cannot connect to cluster: %s\n", strerror(-error));
		
		exit(EXIT_FAILURE);
	}
	
	return cluster;
}

void cluster_shutdown(rados_t *cluster){
	rados_shutdown(cluster);
}

rados_read_op_t create_read_op(void){
	
	return rados_create_read_op();
}

void release_read_op(rados_read_op_t *read_op){
	
	rados_release_read_op(read_op);
}

int create_pool(rados_t cluster, const char * pool_name, uint8_t crush_rule_num){
	
	error = rados_pool_create_with_crush_rule(cluster,pool_name,crush_rule_num);
	if(error < 0){
		fprintf(stderr, "[create_pool/ceph_lib.c] Cannot create directory: %s\n", strerror(-error));
		
		return 1;
	}
	
	return 0;
}

//get_object_size
int get_object_size(rados_ioctx_t io_ctx,char *obj_id,uint64_t *len_bucket,time_t *mtime){
	
	error = rados_stat(io_ctx, obj_id, len_bucket, mtime);
	if(error < 0){
		fprintf(stderr, "[get_object_size/ceph_lib.c] Cannot read object size: %s\n", strerror(-error));
		
		return 1;
	}
	
	return 0;
	
}

int remove_pool(rados_t cluster, const char * pool_name){
	
	error = rados_pool_delete(cluster,pool_name);
	if(error < 0){
		fprintf(stderr, "[remove_pool/ceph_lib.c] Cannot remove directory: %s\n", strerror(-error));
		
		return 1;
	}
	
	return 0;
}

rados_ioctx_t create_ioctx(rados_t cluster, char *poolname){

	rados_ioctx_t io_ctx;
	
	error =  rados_ioctx_create(cluster, poolname, &io_ctx);
	if(error < 0)
		fprintf(stderr, "[create_ioctx/ceph_lib.c] Cannot open rados pool : %s\n",strerror(-error));
		
	return io_ctx;
	
}

void destroy_ioctx(rados_ioctx_t *io_ctx){
	rados_ioctx_destroy(io_ctx);
}

rados_completion_t create_completion(rados_completion_t *comp){
	
	error =  rados_aio_create_completion(NULL, NULL, NULL, comp);
	if(error < 0)
		fprintf(stderr, "[create_completion/ceph_lib.c] Could not create aio completion: %s\n",strerror(-error));
		
	return comp;

}

int read_object(rados_ioctx_t io_ctx,char *obj_id,char *read_res,size_t len_buff,uint64_t offset,int async){

	/*
	 * Read data from the cluster asynchronously.
	 * First, set up asynchronous I/O completion.
	 */
	rados_completion_t comp;
	int ret_oper;
	
	if(async){
	  comp = create_completion(&comp);
	
	  /* Next, read data using rados_aio_read. */
	  error =  rados_aio_read(io_ctx,obj_id,comp,read_res,len_buff,offset);
	  if(error >= 0){
		  /* Wait for the operation to complete */
	    rados_aio_wait_for_complete(comp);
	    ret_oper = rados_aio_get_return_value(comp); //retorno apos o tempo de espera 
			if(ret_oper < 0){
				fprintf(stderr, "[read_object/ceph_lib.c] Cannot read object. %s\n", strerror(-error));
				rados_aio_release(comp);
				
				return 1; //retorna 1 indicando que operacao falhou
			}
	  }
		/* Release the asynchronous I/O complete handle to avoid memory leaks. */
	  rados_aio_release(comp);		
	}else
	  error =  rados_read(io_ctx,obj_id,read_res,len_buff,offset);	
	
	if(error < 0){
	  fprintf(stderr, "[read_object/ceph_lib.c] Cannot read object. %s\n", strerror(-error));
		
		return 1; // indica para iceph.c que ocorreu erro 
	}
	
	return 0; //retorna 0 indicando que operacao foi bem sucedida
}

int write_object(rados_ioctx_t io_ctx,char *obj_id,char *buffer,size_t len_buff,uint64_t offset,int async){
	
	rados_completion_t comp;
	int ret_oper;
	
	if(async){
		comp = create_completion(&comp);
	
		error =  rados_aio_write(io_ctx, obj_id,comp,buffer,len_buff, 0);
		if(error >= 0){
			rados_aio_wait_for_complete(comp);
	    ret_oper = rados_aio_get_return_value(comp); //retorno apos o tempo de espera 
			if(ret_oper < 0){
				fprintf(stderr, "[read_object/ceph_lib.c] Cannot write object. %s\n", strerror(-error));
				rados_aio_release(comp);
				
				return 1; //retorna 1 indicando que operacao falhou
			}
		}	
	}else
		error =  rados_write(io_ctx, obj_id, buffer, len_buff, offset);
	
	if(error < 0){
		fprintf(stderr,"[write_object/ceph_lib.c] Cannot write object. %s \n", strerror(-error));
		
		return 1;
	}	
	
	return 0;
}

int write_object_full(rados_ioctx_t io_ctx, char *obj_id, char *buffer, size_t len_buff){
	
	error =  rados_write_full(io_ctx, obj_id, buffer, len_buff);
	if(error < 0 ) {
		fprintf(stderr,"[write_object_full/ceph_lib.c] Cannot write object. %d\n", strerror(-error));
		
		return 1;
	}
	
	return 0;
}

int remove_object(rados_ioctx_t io_ctx,char *obj_id){

	error =  rados_remove(io_ctx, obj_id);
	if(error < 0 ){
		fprintf(stderr,"[remove_object/ceph_lib.c] cannot remove object. <%d>\n", strerror(-error));
		
		return 1;
	}
	
	return 0;
}