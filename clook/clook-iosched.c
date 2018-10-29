//elevator noop 
//noop-iosched.c
//modified to perform a clook algrithm

#include <linux/blkdev.h>
#include <linux/elevator.h>
#include <linux/bio.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>

// set default val
int diskhead = -1;

//struct noop_data {
//	struct list_head queue;
//};

struct clook_data {
	struct list_head queue;
};

static void clook_merged_requests(struct request_queue *q, struct request *rq, struct request *next) {
	list_del_init(&next->queuelist);	// delete
	elv_dispatch_sort(q, next);			// sort
}

static int clook_dispatch(struct request_queue *q, int force) {
	struct clook_data *nd = q->elevator->elevator_data;
	char direction;

	if (!list_empty(&nd->queue)) {
		struct request *rq;
		rq = list_entry(nd->queue.next, struct request, queuelist);
		list_del_init(&rq->queuelist);
		elv_dispatch_sort(q, rq);
		
		// locate diskhead
		diskhead = blk_rq_pos(rq);

		// if request is read or write
		
		if (rq_data_dir(rq) == READ) direction = 'R';
		else direction = 'W';
		printk("[CLOOK] dsp %c %lu\n", direction, (long unsigned)blk_rq_pos(rq));

		return 1;
	}
	return 0;
}

static void clook_add_request(struct request_queue *q, struct request *rq) {
	struct clook_data *nd = q->elevator->elevator_data;
	struct list_head *cur = NULL;
	char direction;

	list_for_each(cur, &nd->queue)
	{
		struct request *c = list_entry(cur, struct request, queuelist);
		
		// find cur smaller than diskhead and greater than request
		if (blk_rq_pos(rq) > diskhead)
		{
			// if cur position less than diskhead
			// or request position less than cur position
			// add cur position
			if(blk_rq_pos(c) < diskhead || blk_rq_pos(rq) < blk_rq_pos(c))
				break;
			
		} 
		else {
			// if cur position less than diskhead
			// and request position less than cur position
			// add cur position
			if(blk_rq_pos(c) < diskhead && blk_rq_pos(rq) < blk_rq_pos(c))
				break;
		}
	}

	// if request is read or write
	
	if (rq_data_dir(rq) == READ) direction = 'R';
	else direction = 'W';
	printk("[CLOOK] add %c %lu\n", direction, (long unsigned)blk_rq_pos(rq));

	// add cur
	list_add_tail(&rq->queuelist, cur);
}

//static int clook_queue_empty(struct request_queue *q) {
//	struct clook_data *nd = q->elevator->elevator_data;
//
//	return list_empty(&nd->queue);
//}

static struct request *clook_former_request(struct request_queue *q, struct request *rq) {
	struct clook_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.prev == &nd->queue) return NULL;

	return list_entry(rq->queuelist.prev, struct request, queuelist);
}

static struct request *clook_latter_request(struct request_queue *q, struct request *rq) {
	struct clook_data *nd = q->elevator->elevator_data;

	if (rq->queuelist.next == &nd->queue) return NULL;
	
	return list_entry(rq->queuelist.next, struct request, queuelist);
}

static int clook_init_queue(struct request_queue *q, struct elevator_type *e) {
	struct clook_data *nd;
	struct elevator_queue *eq;
	
	eq = elevator_alloc(q, e);
	if(!eq)
		return -ENOMEM;
	
	nd = kmalloc_node(sizeof(*nd), GFP_KERNEL, q->node);
	if (!nd) {
		kobject_put(&eq->kobj);
		return -ENOMEM;
	}
	eq->elevator_data = nd;
	
	INIT_LIST_HEAD(&nd->queue);
	
	spin_lock_irq(q->queue_lock);
	q->elevator = eq;
	spin_unlock_irq(q->queue_lock);
	
	return 0;
}

static void clook_exit_queue(struct elevator_queue *e) {
	struct clook_data *nd = e->elevator_data;

	BUG_ON(!list_empty(&nd->queue));
	kfree(nd);
}

static struct elevator_type elevator_clook = {
	.ops = {
		.elevator_merge_req_fn		= clook_merged_requests,
		.elevator_dispatch_fn		= clook_dispatch,
		.elevator_add_req_fn		= clook_add_request,
		//.elevator_queue_empty_fn	= clook_queue_empty,
		.elevator_former_req_fn		= clook_former_request,
		.elevator_latter_req_fn		= clook_latter_request,
		.elevator_init_fn		= clook_init_queue,
		.elevator_exit_fn		= clook_exit_queue,
	},
	.elevator_name = "clook",
	.elevator_owner = THIS_MODULE,
};

static int __init clook_init(void) {
	return elv_register(&elevator_clook);
}

static void __exit clook_exit(void) {
	elv_unregister(&elevator_clook);
}

module_init(clook_init);
module_exit(clook_exit);

MODULE_AUTHOR("Jiawei Mo");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("clook IO Scheduler");