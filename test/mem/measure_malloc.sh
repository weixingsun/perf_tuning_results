
SECONDS=10
export LD_LIBRARY_PATH=/usr/local/lib/bcc:$LD_LIBRARY_PATH
/home/sun/jbb/bcc/tools/funclatency -d $SECONDS kmalloc_slab > funclatency_kmalloc_slab.log &
/home/sun/jbb/bcc/tools/funccount   -d $SECONDS kmalloc_slab > funccount_kmalloc_slab.log &
/home/sun/jbb/bcc/tools/funclatency -d $SECONDS __kmalloc > funclatency_kmalloc.log &
/home/sun/jbb/bcc/tools/funccount   -d $SECONDS __kmalloc > funccount_kmalloc.log &
/home/sun/jbb/bcc/tools/argdist -n 3 -d $SECONDS -H 'p::__kmalloc(size_t size):size_t:size' > func_arg_kmalloc.log &
